from setuptools import setup, find_packages

setup(
    name='my_streamlit_app',
    version='0.1',
    packages=find_packages(),
    install_requires=[
        'streamlit==1.36.0',
        'pandas==2.2.2',
        'numpy==1.26.4',
        'scikit-learn==1.5.0',
        'matplotlib==3.8.4',
        'tensorflow==2.17.0',
        'requests==2.32.3',
        'Flask==3.0.3',
        'opencv-python==4.10.0.84',
        'beautifulsoup4==4.12.3'
    ],
)