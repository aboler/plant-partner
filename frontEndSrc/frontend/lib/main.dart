import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';
import 'package:frontend/routes.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

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
      theme: ThemeData(
        elevatedButtonTheme: ElevatedButtonThemeData(
          style: ElevatedButton.styleFrom(
            foregroundColor: Colors.black,
            padding: const EdgeInsets.symmetric(
              horizontal: 24, vertical: 12),
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(12),
          ),
          )
        )
      )
      
    );
  }
}

//GET API request
Future<dynamic> dataBackend() async {}

//POST API req
Future<dynamic> createPost(String api) async{}

//PUT API req
Future<dynamic> put(String api) async{}

//DELETE API req
Future<dynamic> deletePost(String api) async{}
