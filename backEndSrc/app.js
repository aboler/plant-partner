const express = require('express');
const app = express();
const port = 3000; //change to our desired port

const mongoose = require('mongoose');
mongoose.connect('<my-db-url>', {useNewUrlParser: true, useUnifiedTopology : true});
db.on('error', console.error.bind(console, 'MongoDB connection error:'));
db.once('open', () => {console.log('Connected to MongoDB')});

app.get('/items', (req, res) => {res.json({message: 'Get all items'})});