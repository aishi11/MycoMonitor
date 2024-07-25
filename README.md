# MycoMonitor

MycoMonitor is a web application designed to monitor temperature, humidity, and motion, as well as to determine mushroom growth stages by uploading photos. The system provides an intuitive interface for cultivators to track environmental conditions and optimize their mushroom cultivation process.

## Features

- Real-time monitoring of temperature, humidity, and motion
- Image upload and classification of mushroom growth stages (Miselia, Primordia, Tubuh Jamur, Panen)
- User-friendly dashboard for monitoring and analysis
- Notifications for critical environmental changes and growth stages
- Historical data storage and analysis

## Installation

To install MycoMonitor, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/aishi11/mycomonitor.git
    ```
2. Navigate to the project directory:
    ```sh
    cd mycomonitor
    ```
3. Install the required dependencies:
    ```sh
    pip install -r requirements.txt
    ```

## Usage

To use MycoMonitor, follow these steps:

1. Start the monitoring system:
    ```sh
    python MycMOnitor.ino
    ```
2. Run the web application:
    ```sh
    streamlit run MycoWebsite.py
    ```
    ```sh
    python MycoApi.py
    ```
3. Access the dashboard through your web browser at `http://localhost:8501`.

## Requirements

- Python 3.8+
- OpenCV
- TensorFlow or PyTorch
- Streamlit
- Flask
- Blynk

## Contributing

We welcome contributions to MycoMonitor! Please follow these steps to contribute:

1. Fork the repository.
2. Create a new branch:
    ```sh
    git checkout -b feature/your-feature-name
    ```
3. Make your changes and commit them:
    ```sh
    git commit -m "Add your commit message"
    ```
4. Push to the branch:
    ```sh
    git push origin feature/your-feature-name
    ```
5. Open a pull request and describe your changes.

## Authors

- **aishi11** - *Initial work* - [aishi11](https://github.com/aishi11)

Feel free to reach out if you have any questions or suggestions!
