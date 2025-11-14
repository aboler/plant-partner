import mongoose from "mongoose";

const sensorSchema = new mongoose.Schema({
    name: { type: String, required: true },
    time: { type: Number, required: true },
    value: { type: Number, required: true },
});

export default mongoose.model("Sensor", sensorSchema);