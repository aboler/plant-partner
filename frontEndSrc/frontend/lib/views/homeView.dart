import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:frontend/post.dart';
import 'package:frontend/services/remote_service.dart';

class HomeView extends StatefulWidget {
  const HomeView({super.key});

  @override
  State<HomeView> createState() => _HomeViewState();
}

class _HomeViewState extends State<HomeView> {
  List<dynamic>? posts;
  //api response is loaded or not
  var isLoaded = false;
  String plantName = '';
  String sunflower= '';
  @override
  void initState() {
    super.initState();

    //fetch data from api
    getData();
  }

  getData() async {
    posts = await RemoteService().getPlant();
    print(posts);
    if (posts != null) {
      plantName = jsonEncode(posts);
      print(plantName);
      print('converted to string');
      setState(() {
        isLoaded = true;
      });
    }
  }
  
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Home'),
        backgroundColor: Colors.lightGreen,
      ),
      backgroundColor: const Color.fromARGB(255, 255, 248, 237),
      body: Visibility(
        visible: isLoaded,
        child: Text(plantName),
        replacement: CircularProgressIndicator(),
        // child: ListView.builder(
        //   itemCount: 1,
        //   itemBuilder: (context, index) {
        //     return Container(child: Text(posts!.plantName));
        //   },
        // ),
      ),
      
      
      // body: Center(
      //   child: Column(
      //     mainAxisAlignment: MainAxisAlignment.center,
      //     children: [
      //       const Text(
      //         "No Plants Currently",
      //         style: TextStyle(fontSize: 18, fontStyle: FontStyle.italic),
      //       ),
      //       const SizedBox(height: 24),
      //       ElevatedButton.icon(
      //         onPressed: () {
      //           Navigator.pushNamed(context, '/addPlant');
      //         },
      //         icon: const Icon(Icons.add),
      //         label: const Text("Add New Plant"),
      //         style: ElevatedButton.styleFrom(
      //           backgroundColor: Colors.lightGreen,
      //         ),
      //       ),
      //     ],
      //   ),
      // ),
    );
  }
}

// class HomeView extends StatelessWidget {
//   const HomeView({super.key});

//   @override
//   Widget build(BuildContext context) {
//     return Scaffold(
//       appBar: AppBar(
//         title: const Text('Home'),
//         backgroundColor: Colors.lightGreen,
//       ),
//       backgroundColor: const Color.fromARGB(255, 255, 248, 237),
//       body: Center(
//         child: Column(
//           mainAxisAlignment: MainAxisAlignment.center,
//           children: [
//             const Text(
//               "No Plants Currently",
//               style: TextStyle(fontSize: 18, fontStyle: FontStyle.italic),
//             ),
//             const SizedBox(height: 24),
//             ElevatedButton.icon(
//               onPressed: () {
//                 Navigator.pushNamed(context, '/addPlant');
//               },
//               icon: const Icon(Icons.add),
//               label: const Text("Add New Plant"),
//               style: ElevatedButton.styleFrom(
//                 backgroundColor: Colors.lightGreen,
//               ),
//             ),
//           ],
//         ),
//       ),
//     );
//   }
// }
