import pytest
from brdf_loader import *
from pathlib import Path

test_dir = Path(__file__).parent

@pytest.fixture
def bsdf():
    filename = test_dir / "colodur_connemara_4c_rgb.bsdf"
    if not filename.exists():
        import urllib.request
        urllib.request.urlretrieve("http://rgl.s3.eu-central-1.amazonaws.com/media/materials/colodur_connemara_4c/colodur_connemara_4c_rgb.bsdf", str(filename))
    return str(filename)

def test_main(bsdf):
    brdf = BRDF_rgb(bsdf)
    print(brdf.eval(Vector3f(1.), Vector3f(1.)))
    brdf.eval(Vector3f((1, 0, 0)), Vector3f((1, 0, 0)))

    rgb, wo, pdf = brdf.sample(Vector2f(0.5, 0.1), Vector3f(1.))
    print(rgb, wo, pdf)
    assert False
