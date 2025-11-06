import 'package:flutter/material.dart';
import 'package:frontend/plantDetailsView.dart';
import 'package:frontend/addPlantView.dart';

Map<String, WidgetBuilder> appRoutes = {
  '/addPlant': (context) => const AddPlantView(),
  '/plantDetails': (context) => const PlantDetailsView(plantName: "Example Plant"),
};

