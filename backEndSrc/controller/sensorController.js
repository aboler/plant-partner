import Sensor from "../model/sensorModel.js";

export const createSensor = async (req, res) => {
    try {
        const sensorData = new Sensor(req.body);
        const { name } = sensorData;

        const sensorExist = await Sensor.findOne({ name });
        if (sensorExist) {
            return res.status(400).json({ message: 'Sensor data already exists' });
        }

        const savedSensor = await sensorData.save();
        res.status(200).json(savedSensor);
    } catch (error) { 
        res.status(400).json({ error: 'Failed to create sensor data' });
    }
};

export const fetchSensors = async (req, res) => {
    try {
        const sensorData = await Sensor.find();
        res.status(200).json(sensorData);
    } catch (error) {
        res.status(500).json({ error: 'Failed to fetch sensor data' });
    }       
};