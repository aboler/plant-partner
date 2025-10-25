import 'package:flutter/material.dart';
import 'package:frontend/nav.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
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

