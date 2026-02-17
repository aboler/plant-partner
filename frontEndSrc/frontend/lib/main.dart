import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';
import 'package:frontend/routes.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

//main app dev
void main() {
  runApp(const MyApp());
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

//GET API request
Future<dynamic> dataBackend() async {}

//POST API req
//dynamic bcus it can be any data
Future<dynamic> createPost(String api) async{}

//PUT API req
Future<dynamic> put(String api) async{}

//DELETE API req
Future<dynamic> deletePost(String api) async{}
