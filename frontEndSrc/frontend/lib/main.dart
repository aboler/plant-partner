import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';
import 'package:frontend/routes.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(const MyApp());
  fetchDataFromBackend();
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      routes: appRoutes,
      home: Nav(),
      
      // home: Scaffold(
      //   backgroundColor: Color.fromARGB(255, 255, 248, 237),
      //   appBar: AppBar(
      //     title: Text("Dashboard"),
      //     backgroundColor:  const Color.fromARGB(255, 186, 255, 161),
      //   ),
      //   bottomNavigationBar: BottomAppBar(
      //     child: IconButton(onPressed: (){}, icon: Icon(Icons.home)),
      //     color: Colors.lightGreen,
        
      //   ),
      // ),
    );
  }
}

//GET API request
Future<void> fetchDataFromBackend() async {
  //todo: port forwarding this is the wireless wifi ip lan address
  final resp = await http.get(Uri.parse('http://10.136.159.184:8000/api/sensor/fetchSensors'));

  if (resp.statusCode == 200)
  {
    final data = json.encode(resp.body);
    print('received data: $data');

  }
  else {
    print('failed to load data: ${resp.statusCode}');
  }
}

//POST API req

//PUT API req

//DELETE API req
