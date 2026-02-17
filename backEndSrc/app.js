import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";
import router from "./routes/plantRoute.js";
import taskRouter from "./routes/taskRoute.js";
import cors from "cors";
import mqtt from "mqtt";
import http from "http";

// MQTT Broker Setup
const MQTT_BROKER_URL = process.env.MQTT_BROKER_URL || 'mqtt://localhost:1883';
const mqttClient = mqtt.connect(MQTT_BROKER_URL);

mqttClient.on('connect', () => {
    console.log('Connected to MQTT Broker');
    mqttClient.subscribe('sensors/updates', (err) => {
        if (!err) {
            console.log('Subscribed to sensors/updates topic');
        } else {
            console.error('Subscription error:', err);
        }
    });
});

mqttClient.on('message', (topic, message) => {
    console.log(`Received message on topic ${topic}: ${message.toString()}`);
    // Here you can add logic to process the message and update the database if needed
});

// Backend Server Setup
const app = express();
app.use(cors());
app.use(bodyParser.json());
app.use(express.json());
dotenv.config();

const PORT = process.env.PORT || 8000; //change to our desired port or if desired port changes
const MONGOURL = process.env.MONGO_URL || 'mongodb+srv://ncimino1_db_user:BKC89YKbMscuwW8A@plantpartnercluster.mfa1ope.mongodb.net/SensorDatabase?retryWrites=true&w=majority';

// MongoDB Connection

mongoose.connect(MONGOURL).then(() => {
    console.log('Connected to MongoDB')
    app.listen(PORT, () => {
        console.log(`Server running on port ${PORT}`);
    });
}).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
});

app.use("/sensors", route);
app.use("/plants", router);
app.use("/tasks", taskRouter);