from flask import Flask, jsonify, request

app = Flask(__name__)

# Global variables to hold sensor data
sensor_data = {
    "temperature": 0,
    "humidity": 0,
    "motion": 0,
}

@app.route('/update', methods=['GET'])
def update_data():
    global sensor_data
    sensor_data['temperature'] = float(request.args.get('temperature', 0))
    sensor_data['humidity'] = float(request.args.get('humidity', 0))
    sensor_data['motion'] = int(request.args.get('motion', 0))
    return "Data updated", 200

@app.route('/get_sensor_data', methods=['POST'])
def get_sensor_data():
    global sensor_data
    data = request.form  # Get data from POST request
    sensor_data['temperature'] = float(data['temperature'])
    sensor_data['humidity'] = float(data['humidity'])
    sensor_data['motion'] = int(data['motion'])
    return jsonify({'message': 'Data received successfully'})

@app.route('/send_sensor_data', methods=['GET'])
def send_sensor_data():
    global sensor_data
    return jsonify(sensor_data)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)  # Run Flask app
