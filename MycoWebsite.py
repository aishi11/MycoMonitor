import subprocess
import time
import streamlit as st
import requests
import cv2
import numpy as np
import matplotlib.pyplot as plt
import os
from keras.models import load_model
from PIL import Image

# Start Flask server
flask_process = subprocess.Popen(['python', 'MycoApi.py'])

# Allow Flask some time to start
time.sleep(5)

# Load the trained model
model_path = 'mushroom_growth_classifier.h5'
try:
    model = load_model(model_path)
    st.success("Model loaded successfully!")
except Exception as e:
    st.error(f"Error loading model: {e}")

# Fungsi untuk preprocessing gambar
def preprocess_image(image):
    try:
        resized_image = cv2.resize(image, (224, 224))  # Sesuaikan ukuran sesuai dengan model
        normalized_image = resized_image / 255.0
        preprocessed_image = np.expand_dims(normalized_image, axis=0)
        return preprocessed_image
    except Exception as e:
        st.error(f"Error during image preprocessing: {e}")
        return None

# Fungsi untuk melakukan prediksi
def predict_from_frame(model, image):
    preprocessed_image = preprocess_image(image)
    if preprocessed_image is not None:
        try:
            st.write("Preprocessed Image Shape:", preprocessed_image.shape)
            prediction = model.predict(preprocessed_image)
            st.write("Prediction:", prediction)
            label = np.argmax(prediction)
            confidence = np.max(prediction)
            return label, confidence
        except Exception as e:
            st.error(f"Error during prediction: {e}")
            return None, None
    else:
        return None, None

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
    
    if label_index is not None:
        label = labels[label_index]  # Dapatkan nama label dari indeks
        # Display the result
        st.image(image_rgb, caption=f'Tahapan Jamur: {label}, Confidence: {confidence:.2f}', use_column_width=True)
    else:
        st.error("Failed to predict the mushroom growth stage.")

# Fungsi untuk menampilkan data sensor
def display_sensor_data():
    sensor_data = fetch_sensor_data()
    if sensor_data:
        st.header("Sensor Data")
        st.write(f"Temperature: {sensor_data.get('temperature')} °C")
        st.write(f"Humidity: {sensor_data.get('humidity')} %")
        st.write(f"Motion: {'Detected' if sensor_data.get('motion') == 1 else 'Not Detected'}")
    else:
        st.write("No sensor data available.")

# Refresh data setiap interval waktu tertentu
if st.button("Start Data Refresh"):
    while True:
        display_sensor_data()
        st.text(f"Refreshing data every {refresh_interval} seconds...")
        time.sleep(refresh_interval)
        st.experimental_rerun()
else:
    display_sensor_data()
