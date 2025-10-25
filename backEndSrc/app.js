import express from "express"
import mongoose from "mongoose"
import dotenv from "dotenv";

const app = express();
dotenv.config();

const PORT = process.env.PORT || 3000; //change to our desired port
const MONGOURL = process.env.MONGO_URL;


mongoose.connect(MONGOURL).then(() => {
    console.log('Connected to MongoDB')
    app.listen(PORT, () => {
        console.log(`Server running on port ${PORT}`);
    });
}).catch((error) => {
    console.error('Error connecting to MongoDB:', error);
});

const sensorSchema = new mongoose.Schema({
    id: String,
    Time: Number,
    Value: Number,
});

const Sensor = mongoose.model('sensors', sensorSchema)

app.get('/get-sensor-data', async (req, res) => {
    const sensorData = await Sensor.find();
    res.json(sensorData);
});
    
//const db = mongoose.connection;
//db.on('error', console.error.bind(console, 'MongoDB connection error:'));
//db.once('open', () => {console.log('Connected to MongoDB')});

//const { MongoClient, ServerApiVersion } = require('mongodb');
//const uri = "mongodb+srv://Nicholas_Ci:rY6MLdu4uAzEDagm@plantpartnercluster.mgobjwu.mongodb.net/?appName=PlantPartnerCluster";
// Create a MongoClient with a MongoClientOptions object to set the Stable API version
//const client = new MongoClient(uri, {
//  serverApi: {
//    version: ServerApiVersion.v1,
//    strict: true,
//    deprecationErrors: true,
//  }
//});
//async function run() {
//  try {
//    // Connect the client to the server	(optional starting in v4.7)
//    await client.connect();
//    // Send a ping to confirm a successful connection
//    await client.db("admin").command({ ping: 1 });
//    console.log("Pinged your deployment. You successfully connected to MongoDB!");
//  } finally {
//    // Ensures that the client will close when you finish/error
//    await client.close();
//  }
//}
//run().catch(console.dir);

//app.get('/items', (req, res) => {res.json({message: 'Get all items'})});