export const fetchSensors = async (req, res) => {
    try {
        //const sensorData = await Sensor.find();
        return res.json("Hello World!"); 
        //res.status(200).json(sensorData);
    } catch (error) {
        res.status(500).json({ error: 'Failed to fetch sensor data' });
    }       
};