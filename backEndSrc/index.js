const express = require('express')
const app = express()
const port = 8080 || process.env.port

app.listen(port, ()=>{
   console.log('port running on '+port)
})