#include "io_datareader.h"

#include "../datastructure/DeliveryPoint.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace IO;

bool IOHandler::readDeliveryData(std::string &fileName, std::vector<BASE::DeliveryPoint>& deliveries)
{
   std::fstream file;
   int custNo;
   int xCoord;
   int yCoord;
   int demand;
   int readyTime;
   int dueDate;
   int serviceTime;
   int numberOfVehicle;
   int capacity;

   // opening file 
   file.open(fileName.c_str());
   if (!file.is_open())
      return false;
   file >> numberOfVehicle >> capacity;
   // extracting delivery data from file.
   deliveries.clear();
   while (file >> custNo >> xCoord >> yCoord >> demand >> readyTime >> dueDate >> serviceTime)
   {
      deliveries.emplace_back(custNo, xCoord, yCoord, demand, readyTime, dueDate, serviceTime, BASE::DeliveryType::DELIVERY);
   }
   file.close();

   return true;
}

bool IOHandler::readVehiclesData(std::string &fileName, int &numberOfVehicles, int &capacity)
{
   std::fstream file;

   // opening file and reading
   file.open(fileName.c_str());
   if (!file.is_open())
      return false;

   file >> numberOfVehicles >> capacity;
   file.close();

   return true;
}

