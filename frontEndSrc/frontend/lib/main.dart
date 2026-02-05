import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';
import 'package:frontend/routes.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

//main app dev
void main() {
  runApp(const MyApp());
  dataBackend();
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      routes: appRoutes,
      home: Nav(),
      
    );
  }
}

//const String baseUrl = 'http://#:8000/plants/getPlantByName/Sunflower';
const String baseUrl = 'http://10.0.2.2:8000/plants/getPlantByName/Sunflower';

//GET API request
Future<dynamic> dataBackend() async {
  //todo: port forwarding this is the wireless wifi ip lan address
  final resp = await http.get(Uri.parse(baseUrl));

  if (resp.statusCode == 200)
  {
    final data = json.decode(resp.body);
    print('received data: $data');
    return data;
  }
  else {
    print('failed to load data: ${resp.statusCode}');
  }
}

//POST API req
//dynamic bcus it can be any data
Future<dynamic> createPost(String api) async{}

//PUT API req
Future<dynamic> put(String api) async{}

//DELETE API req
Future<dynamic> deletePost(String api) async{}
