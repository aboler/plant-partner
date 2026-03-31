import mongoose from "mongoose";

const plantSchema = new mongoose.Schema({
    plantName: { type: String, required: true },
    soilMoisture: { type: Number, required: true },
    lightIntensity: { type: Number, required: true },
    nLevel: { type: Number, required: true },
    pLevel: { type: Number, required: true },
    kLevel: { type: Number, required: true },
    autoSchedule: { type: Boolean, default: false },
});

export default mongoose.model("Plant", plantSchema);