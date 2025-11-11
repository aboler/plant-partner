import 'package:flutter/material.dart';

class HomeView extends StatefulWidget {
  const HomeView({super.key});

  @override
  State<HomeView> createState() => _HomeViewState();
}

class _HomeViewState extends State<HomeView> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Home'),
        backgroundColor: Colors.lightGreen,
      ),
      backgroundColor: const Color.fromARGB(255, 255, 248, 237),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            const Text(
              "No Plants Currently",
              style: TextStyle(fontSize: 18, fontStyle: FontStyle.italic),
            ),
            const SizedBox(height: 24),
            ElevatedButton.icon(
              onPressed: () {
                Navigator.pushNamed(context, '/addPlant');
              },
              icon: const Icon(Icons.add),
              label: const Text("Add New Plant"),
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.lightGreen,
              ),
            ),
          ],
        ),
      ),
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
