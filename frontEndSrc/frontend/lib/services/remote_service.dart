import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:frontend/plant.dart';
import 'dart:convert';

//const String baseUrl = 'http://10.###.###.###:8000/plants/getPlantByName/Sunflower';
//const String baseUrl = 'http://10.0.2.2:8000/plants/getPlantByName/Sunflower';
const String baseUrl = 'http://172.20.10.7:8000/plants/getPlantByName/Sunflower';
class RemoteService {
  Future<Plant?> getPlant() async {
    final resp = await http.get(Uri.parse(baseUrl));

    if (resp.statusCode == 200) {
      print('made it thru');
      final List data = json.decode(resp.body);

      if (data.isNotEmpty) {
        return Plant.fromJson(data[0]); // since list contains one plant
      }
    } else {
      print('failed: ${resp.statusCode}');
    }

    return null;
  }
}

Future<bool> setAutoSchedule(bool enabled) async {
  final resp = await http.put(
    Uri.parse('$baseUrl/autoSchedule'),
    headers: {'Content-Type': 'application/json'},
    body: jsonEncode({'autoSchedule': enabled}),
  );

  return resp.statusCode == 200;
}