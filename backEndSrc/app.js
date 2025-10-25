import express from "express";
import mongoose from "mongoose";
import dotenv from "dotenv";
import bodyParser from "body-parser";
import route from "./routes/sensorRoute.js";

const app = express();
app.use(bodyParser.json());
app.use(express.json());
dotenv.config();

const PORT = process.env.PORT || 8000; //change to our desired port
const MONGOURL = process.env.MONGO_URL || 'mongodb://localhost:27017/SensorDatabase';


mongoose.connect(MONGOURL).then(() => {
    console.log('Connected to MongoDB')
    app.listen(PORT, () => {
        console.log(`Server running on port ${PORT}`);
    });
}).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
});

app.use("/api/sensor", route);