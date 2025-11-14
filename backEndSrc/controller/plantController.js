import Plant from "../model/plantModel.js";

export const createPlant = async (req, res) => {
    try {
        const plantData = new Plant(req.body);
        const { plantName } = plantData;

        const plantExist = await Plant.findOne({ plantName });
        if (plantExist) {
            return res.status(400).json({ message: 'Plant data already exists' });
        }

        const savedPlant = await plantData.save();
        res.status(200).json(savedPlant);
    } catch (error) {
        res.status(400).json({ error: 'Failed to create plant data' });
    }
};

export const fetchPlants = async (req, res) => {
    try {
        const plantData = await Plant.find();
        res.status(200).json(plantData);
    } catch (error) {
        res.status(400).json({ error: 'Failed to fetch plant data' });
    }
};

export const getPlantByName = async (req, res) => {
    try {
        const name = req.params.name;
        const plantExist = await Plant.find({ plantName: name });

        if (!plantExist) {
            return res.status(404).json({ message: 'Plant not found' });
        }

        res.status(200).json(plantExist);
    } catch (error) {
        res.status(400).json({ error: 'Failed to fetch plant data' });
    }
};

export const updatePlantByName = async (req, res) => {
    try {
        const name = req.params.name;
        const plantExist = await Plant.find({ plantName: name });

        if (!plantExist) {
            return res.status(404).json({ message: 'Plant not found' });
        }
        const updatePlant = await Plant.findByIdAndUpdate(plantExist[0]._id, req.body, { new: true });
        res.status(200).json(updatePlant);
    } catch (error) {
        res.status(500).json({ error: 'Failed to update plant data' });
    }
};

export const deletePlantByName = async (req, res) => {
    try {
        const name = req.params.name;
        const plantExist = await Plant.find({ plantName: name });

        if (!plantExist) {
            return res.status(404).json({ message: 'Plant not found' });
        }
        await Plant.findByIdAndDelete(plantExist[0]._id);
        res.status(200).json({ message: 'Plant deleted successfully' });
    } catch (error) {
        res.status(500).json({ error: 'Failed to delete plant data' });
    }
};