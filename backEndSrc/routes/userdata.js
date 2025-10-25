const express = require('express')
const router = express.Router()

// Getting all pieces of information
router.get('/', (req, res) => {


})

// Getting one piece of information
router.get('/:id', (req, res) => {
    req.params.id

})

// Creating a piece of information
router.post('/', (req, res) => {

    
})



module.exports = router
