import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:frontend/plant.dart';
import 'dart:convert';

const String baseUrl = 'http://10.136.85.227:8000/plants/getPlantByName/Sunflower';

class RemoteService
{
  Future<List<dynamic>?> getPlant() async {
  //todo: port forwarding this is the wireless wifi ip lan address
  var resp = await http.get(Uri.parse(baseUrl));

  if (resp.statusCode == 200)
  {
    print('made it thru');
    var dataString = resp.body;
    List<dynamic> dataMap = json.decode(dataString);
    print(dataMap);
    //var plantInfo = Post.fromJson(dataMap);

    print('yipppp');
    //received data: "[{\"_id\":\"68fed1313fbfd4545947b6df\",\"name\":\"MoistureSensor\",\"time\":10,\"value\":25,\"__v\":0}]"
    //from the debug console
    
    //Post post = postFromJson(dataString);
    print('hhh');
    return dataMap;
  }
  else {
    print('failed to load data: ${resp.statusCode}');
  }
}
}