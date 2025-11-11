import 'package:flutter/material.dart';
import 'package:frontend/views/plantDetailsView.dart';
import 'package:frontend/views/addPlantView.dart';

Map<String, WidgetBuilder> appRoutes = {
  '/addPlant': (context) => const AddPlantView(),
  '/plantDetails': (context) => const PlantDetailsView(plantName: "Example Plant"),
};

