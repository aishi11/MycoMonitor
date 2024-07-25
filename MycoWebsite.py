import streamlit as st
import requests
import time
import cv2
import numpy as np
import matplotlib.pyplot as plt
import os
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'
from keras.models import load_model
from PIL import Image

# Load the trained model
model_path = 'C:/Users/dell/Documents/Arduino/MycoMOnitor/mushroom_growth_classifier.h5'
model = load_model(model_path)

# Fungsi untuk preprocessing gambar
def preprocess_image(image):
    resized_image = cv2.resize(image, (224, 224))  # Sesuaikan ukuran sesuai dengan model
    normalized_image = resized_image / 255.0
    preprocessed_image = np.expand_dims(normalized_image, axis=0)
    return preprocessed_image

# Fungsi untuk melakukan prediksi
def predict_from_frame(model, image):
    preprocessed_image = preprocess_image(image)
    prediction = model.predict(preprocessed_image)
    label = np.argmax(prediction)
    confidence = np.max(prediction)
    return label, confidence

# Label tahapan jamur (sesuaikan dengan kelas pada model)
labels = ['Miselia', 'Primordia', 'Tubuh Jamur', 'Panen']

# Fungsi untuk fetch data sensor dari Flask server
def fetch_sensor_data():
    try:
        url = "http://192.168.100.12:5000/send_sensor_data"  # Ganti dengan URL Flask server Anda
        response = requests.get(url)
        data = response.json()
        return data
    except Exception as e:
        st.error(f"Error fetching data: {e}")
        return None

# Streamlit app code
st.title("Real-time Sensor Data and Mushroom Growth Classification")

# Add a slider to control the refresh interval
refresh_interval = st.sidebar.slider("Refresh interval (seconds)", 1, 10, 2)

# Upload image
uploaded_file = st.file_uploader("Choose an image...", type=["jpg", "jpeg", "png"])

if uploaded_file is not None:
    # Convert the file to an OpenCV image
    file_bytes = np.asarray(bytearray(uploaded_file.read()), dtype=np.uint8)
    image = cv2.imdecode(file_bytes, 1)
    image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Preprocess and predict
    label_index, confidence = predict_from_frame(model, image_rgb)
    label = labels[label_index]  # Dapatkan nama label dari indeks

    # Display the result
    st.image(image_rgb, caption=f'Tahapan Jamur: {label}, Confidence: {confidence:.2f}', use_column_width=True)

# Main display loop for sensor data
while True:
    sensor_data = fetch_sensor_data()

    if sensor_data:
        st.header("Sensor Data")
        st.write(f"Temperature: {sensor_data.get('temperature')} °C")
        st.write(f"Humidity: {sensor_data.get('humidity')} %")
        st.write(f"Motion: {'Detected' if sensor_data.get('motion') == 1 else 'Not Detected'}")

    st.text(f"Refreshing data every {refresh_interval} seconds...")
    st.text("Press Ctrl+C to stop the Streamlit app.")

    # Add a small delay to reduce CPU usage
    time.sleep(refresh_interval)
    st.experimental_rerun()
