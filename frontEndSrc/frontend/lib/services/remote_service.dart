import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:frontend/plant.dart';
import 'dart:convert';

const String baseUrl = 'http://10.136.166.196:8000/plants/getPlantByName/Sunflower';

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