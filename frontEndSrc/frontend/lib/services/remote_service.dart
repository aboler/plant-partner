import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:frontend/post.dart';

const String baseUrl = 'http://10.136.117.191:8000/plants/getPlantByName/Sunflower';

class RemoteService
{
  Future<Post?> fetchDataFromBackend() async {
  //todo: port forwarding this is the wireless wifi ip lan address
  var resp = await http.get(Uri.parse(baseUrl));

  if (resp.statusCode == 200)
  {
    //final data = json.encode(resp.body);
    print('yipppp');
    //received data: "[{\"_id\":\"68fed1313fbfd4545947b6df\",\"name\":\"MoistureSensor\",\"time\":10,\"value\":25,\"__v\":0}]"
    //from the debug console

    var json = resp.body;
    return postFromJson(json);

  }
  else {
    print('failed to load data: ${resp.statusCode}');
  }
}
}