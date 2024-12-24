# IoT-Remote-healthcare-monitoring-system-ESP32
The proposed system integrates a DS18B20 and SEN-11574 with an ESP-WROOM-32 microcontroller to monitor heart rate and body temperature. The data is displayed on an LCD and transmitted to ThingSpeak for remote access. Readings from ThingApeak will be displayed on a web application.

dataset_collection.ino: This code is used to collect the dataset and integrate with excel.
Finalcode.ino: This contains the final code integrated with sensors and thingspeak.
Index.html, Dashboard.html, Predictions.html and Visualisations.html: All the html codes integrated with flask.py for prediction and thingspeak for displaying.
Sensor_realtime.xslx: Dataset collected from the code mentioned.
random_forest_model_final.pkl: Model used to predict the conditions based on the temp and pulse value.
PROTOTYPE_IMAGE.jpg: How we made the final product
Background_image.jpg: Used in web page

Put all these in a folder and create a Thingspeak channel. Relace your write api key in Finalcode and read api key in Prediction.html.
