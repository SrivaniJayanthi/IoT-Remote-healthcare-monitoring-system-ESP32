from flask import Flask, request, jsonify
import joblib
import numpy as np
import requests
from flask_cors import CORS  # Import CORS

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# Load the trained model
model = joblib.load('random_forest_model_final.pkl')

# Function to get the latest temperature and blood pressure values from ThingSpeak
def fetch_data_from_thingspeak():
    THINGSPEAK_CHANNEL_ID = '2725040'
    THINGSPEAK_READ_API_KEY = 'NP5TTIM90G5TK188'

    # Fetch temperature
    temp_response = requests.get(f'https://api.thingspeak.com/channels/{THINGSPEAK_CHANNEL_ID}/fields/1/last.json?api_key={THINGSPEAK_READ_API_KEY}')
    temp_data = temp_response.json()
    temperature = float(temp_data['field1']) if temp_data['field1'] else None

    # Fetch blood pressure
    bp_response = requests.get(f'https://api.thingspeak.com/channels/{THINGSPEAK_CHANNEL_ID}/fields/2/last.json?api_key={THINGSPEAK_READ_API_KEY}')
    bp_data = bp_response.json()
    bp = float(bp_data['field2']) if bp_data['field2'] else None

    return temperature, bp

# Define the prediction route
@app.route('/predict', methods=['GET'])
def predict():
    # Get the data from ThingSpeak
    temperature, bp = fetch_data_from_thingspeak()

    # Check if the values are valid
    if temperature is None or bp is None:
        return jsonify({'error': 'Invalid data from ThingSpeak'}), 400

    # Prepare the features for prediction
    features = np.array([[temperature, bp]])

    # Make the prediction
    prediction = model.predict(features)
    
    # Mapping of prediction to condition
    condition_mapping = {0: 'Healthy', 1: 'Hypothermia', 2: 'Fever & Hypotension', 3: 'Fever & Hypertension'}
    predicted_condition = condition_mapping.get(prediction[0], 'Unknown')

    # Log the values before returning
    print(f"Temperature: {temperature}, Blood Pressure: {bp}, Predicted Condition: {predicted_condition}")

    return jsonify({
        'temperature': temperature,
        'bloodPressure': bp,
        'predictedCondition': predicted_condition
    })

if __name__ == '__main__':
    app.run(debug=True)
