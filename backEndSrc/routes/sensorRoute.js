import express from "express"
import { fetchSensors, createSensor } from "../controller/sensorController.js";

const route = express.Router();

route.get("/fetchSensors", fetchSensors);
route.post("/createSensor", createSensor);

export default route;

//const express = require('express')
//const router = express.Router()

// Getting all sensor values
//router.get('/', (req, res) => {
//    res.send('Hello World 0!')

//})

// Getting one sensor's information
//router.get('/:id', (req, res) => {
//    req.params.id
//    res.send('Hello World 1!')

//})

// Creating the post of the sensor's information
//router.post('/', (req, res) => {
//    res.send('Hello World 2!')

//})

//Updating a sensor's information
//router.patch('/:id', (req, res) => {
//    res.send('Hello World 3!')
//})

//Deleting a sensor's information
//router.delete('/:id', (req, res) => {
//    res.send('Hello World 4!')
//})


//module.exports = router
