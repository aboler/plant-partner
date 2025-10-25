const express = require('express');
const app = express();
const port = 8000; //change to our desired port

const mongoose = require('mongoose');
mongoose.connect('mongodb+srv://Nicholas_Ci:rY6MLdu4uAzEDagm@plantpartnercluster.mgobjwu.mongodb.net/'
, {useNewUrlParser: true, useUnifiedTopology : true});

const db = mongoose.connection;
db.on('error', console.error.bind(console, 'MongoDB connection error:'));
db.once('open', () => {console.log('Connected to MongoDB')});

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

app.get('/items', (req, res) => {res.json({message: 'Get all items'})});