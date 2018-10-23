import numpy as np
import struct
import os


def size_fmt(num, suffix='B'):
    for unit in ['', 'Ki', 'Mi', 'Gi', 'Ti', 'Pi']:
        if abs(num) < 1024.0:
            return "%3.1f %s%s" % (num, unit, suffix)
        num /= 1024.0
    return "%.1f %s%s" % (num, 'Yi', suffix)


def read_tensor(filename):
    with open(filename, 'rb') as f:
        def unpack(fmt):
            result = struct.unpack(fmt, f.read(struct.calcsize(fmt)))
            return result if len(result) > 1 else result[0]

        if f.read(12) != 'tensor_file\0'.encode('utf8'):
            raise Exception('Invalid tensor file (header not recognized)')

        if unpack('<BB') != (1, 0):
            raise Exception('Invalid tensor file (unrecognized '
                            'file format version)')

        field_count = unpack('<I')
        size = os.stat(filename).st_size
        print('Loading tensor data from \"%s\" .. (%s, %i field%s)'
            % (filename, size_fmt(size),
               field_count, 's' if field_count > 1 else ''))

        # Maps from Struct.EType field in Mitsuba
        dtype_map = {
            1: np.uint8,
            2: np.int8,
            3: np.uint16,
            4: np.int16,
            5: np.uint32,
            6: np.int32,
            7: np.uint64,
            8: np.int64,
            9: np.float16,
            10: np.float32,
            11: np.float64
        }

        fields = {}
        for i in range(field_count):
            field_name = f.read(unpack('<H')).decode('utf8')
            field_ndim = unpack('<H')
            field_dtype = dtype_map[unpack('<B')]
            field_offset = unpack('<Q')
            field_shape = unpack('<' + 'Q' * field_ndim)
            fields[field_name] = (field_offset, field_dtype, field_shape)

        result = {}
        for k, v in fields.items():
            f.seek(v[0])
            result[k] = np.fromfile(f, dtype=v[1],
                                    count=np.prod(v[2])).reshape(v[2])
    return result


def write_tensor(filename, align=8, **kwargs):
    with open(filename, 'wb') as f:
        # Identifier
        f.write('tensor_file\0'.encode('utf8'))

        # Version number
        f.write(struct.pack('<BB', 1, 0))

        # Number of fields
        f.write(struct.pack('<I', len(kwargs)))

        # Maps to Struct.EType field in Mitsuba
        dtype_map = {
            np.uint8: 1,
            np.int8: 2,
            np.uint16: 3,
            np.int16: 4,
            np.uint32: 5,
            np.int32: 6,
            np.uint64: 7,
            np.int64: 8,
            np.float16: 9,
            np.float32: 10,
            np.float64: 11
        }

        offsets = {}
        fields = dict(kwargs)

        # Write all fields
        for k, v in fields.items():
            if type(v) is str:
                v = np.frombuffer(v.encode('utf8'), dtype=np.uint8)
            else:
                v = np.ascontiguousarray(v)
            fields[k] = v

            # Field identifier
            label = k.encode('utf8')
            f.write(struct.pack('<H', len(label)))
            f.write(label)

            # Field dimension
            f.write(struct.pack('<H', v.ndim))

            found = False
            for dt in dtype_map.keys():
                if dt == v.dtype:
                    found = True
                    f.write(struct.pack('B', dtype_map[dt]))
                    break
            if not found:
                raise Exception("Unsupported dtype: %s" % str(v.dtype))

            # Field offset (unknown for now)
            offsets[k] = f.tell()
            f.write(struct.pack('<Q', 0))

            # Field sizes
            f.write(struct.pack('<' + ('Q' * v.ndim), *v.shape))

        for k, v in fields.items():
            # Set field offset
            pos = f.tell()

            # Pad to requested alignment
            pos = (pos + align - 1) // align * align

            f.seek(offsets[k])
            f.write(struct.pack('<Q', pos))
            f.seek(pos)

            # Field data
            v.tofile(f)

        print('Wrote \"%s\" (%s)' % (filename, size_fmt(f.tell())))


def decode_string(a):
    return a.view('S%i' % a.size)[0].decode('utf8')


def plot_tensor(tensor, size=1, normalize_each=True, scale=1, valid=None):
    import matplotlib.pyplot as plt

    def to_srgb(v):
        return np.where(v < 0.0031308, v * 12.92,
                        1.055 * (v**(1 / 2.4)) - 0.055)

    if tensor.ndim == 4:
        tensor = tensor[:, :, np.newaxis, :, :]

    tensor = np.moveaxis(tensor, [0, 1, 2, 3, 4], [1, 0, 4, 2, 3])
    tensor_max = None if normalize_each else np.max(tensor)

    fig, ax = plt.subplots(tensor.shape[1], tensor.shape[0],
                           figsize=(tensor.shape[0] * size,
                                    tensor.shape[1] * size))
    for i in range(tensor.shape[0]):
        for j in range(tensor.shape[1]):
            img = tensor[i, j, ...]
            img_max = np.max(img) if normalize_each else tensor_max
            axc = ax[i] if tensor.shape[1] == 1 else ax[j][i]
            tonemapped = img * (scale / np.maximum(1e-10, img_max))
            tonemapped = to_srgb(np.clip(tonemapped, 0, 1))
            if valid is not None and tonemapped.shape[-1] == 3:
                tonemapped[valid[j, i] == 0] = [1, 0, 0]
            axc.imshow(tonemapped.squeeze(), interpolation='nearest',
                       clim=(0, 1), extent=[0, 1, 0, 1], aspect=1)
            axc.get_xaxis().set_visible(False)
            axc.get_yaxis().set_visible(False)

    axc = ax[0] if tensor.shape[1] == 1 else ax[0][0]
    axc.annotate('$\\theta_i$', xy=(2, 1.15), xytext=(0, 1.1),
                 textcoords='axes fraction', xycoords='axes fraction',
                 arrowprops=dict(arrowstyle="->", connectionstyle="arc3"),
                 zorder=100)

    if tensor.shape[1] > 1:
        axc.annotate('$\\phi_i$', xy=(-0.17, -1), xytext=(-0.25, .85),
                     textcoords='axes fraction', xycoords='axes fraction',
                     arrowprops=dict(arrowstyle="->", connectionstyle="arc3"),
                     zorder=100)

    fig.tight_layout()
    return fig


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    # Read a tensor file from disk
    tensor = read_tensor("vch_ultra_pink_rgb.bsdf")
    print("Description: %s" % decode_string(tensor['description']))
    print("Available fields: %s" % str(list(tensor.keys())))

    valid = np.unpackbits(tensor['valid']).reshape(tensor['luminance'].shape)

    plot_tensor(tensor['vndf'])
    plot_tensor(tensor['rgb'], valid=valid)
    plt.show()

    # Write the tensor file again (the output should be identical)
    write_tensor("out.bsdf", **tensor)
