#include "tspmanager.h"

#include "../datareader/io_datareader.h"
#include "HeuristicTSP.h"
#include "NeighborhoodSearch.h"
#include "helper.h"

SOLVER::TSPManager::TSPManager(std::string fileName)
   : m_fileName{ fileName }
{
}

void SOLVER::TSPManager::runSolvers()
{
   getVehiclesData();
   getDeliveryData();
   markAsDepotAndPickUpPoint();

   SOLVER::HeuristicTSP deliveryManager(m_deliveries, m_capacity, m_numberOfVehicles, m_numberOfPickups);
   auto resultDeliveries = deliveryManager.execute();
   m_numberOfVehicles = resultDeliveries.second;
   m_deliveries = resultDeliveries.first;
   auto resultCategorizedDeliveries = fillCategorizedDeliveries();
   std::cout << "The result from the heuristic algorithm is as follow:" << std::endl;
   HELPER::printPlanning(resultCategorizedDeliveries, m_depot);
   std::cout << std::endl;

   // Running the neighborhood search to make a better result.
   SOLVER::NeighborhoodSearch timeOptimizer(m_deliveries, m_capacity, m_numberOfVehicles, m_numberOfPickups, OBJECTFUNCTION::TIMETRAVELLED);
   resultDeliveries = timeOptimizer.execute();
   m_numberOfVehicles = resultDeliveries.second;
   m_deliveries = resultDeliveries.first;
   resultCategorizedDeliveries = fillCategorizedDeliveries();
   std::cout << "The result from the neighborhood search algorithm with optimization of TIME TRAVELLED is as follow:" << std::endl;
   HELPER::printPlanning(resultCategorizedDeliveries, m_depot);
   std::cout << std::endl;

   SOLVER::NeighborhoodSearch distanceOptimizer(m_deliveries, m_capacity, m_numberOfVehicles, m_numberOfPickups, OBJECTFUNCTION::DISTANCETRAVELLED);
   resultDeliveries = distanceOptimizer.execute();
   m_numberOfVehicles = resultDeliveries.second;
   m_deliveries = resultDeliveries.first;
   resultCategorizedDeliveries = fillCategorizedDeliveries();
   std::cout << "The result from the neighborhood search algorithm with optimization of DISTANCE TRAVELLED is as follow:" << std::endl;
   HELPER::printPlanning(resultCategorizedDeliveries, m_depot);
   std::cout << std::endl;
}


std::vector<std::vector<BASE::DeliveryPoint*>> SOLVER::TSPManager::fillCategorizedDeliveries()
{
   // Transforming the results to categorized deliveries for reporting purpose. 
   m_categorizedDeliveries.clear();
   //std::vector<std::vector<BASE::DeliverPoint*>> resultCategorizedDeliveries;
   for (int i = 0; i < m_numberOfVehicles; i++)
   {
      std::vector<BASE::DeliveryPoint*> categorizedDeliveries{};
      m_categorizedDeliveries.push_back(categorizedDeliveries);
   }

   for (int i = 1; i < m_deliveries.size(); i++)
   {
      auto* deliveryTask = &m_deliveries[i];
      auto* deliveryJob = &m_categorizedDeliveries[deliveryTask->getCategoryNumber() - 1];
      deliveryJob->push_back(deliveryTask);
      m_categorizedDeliveries[deliveryTask->getCategoryNumber() - 1] = *deliveryJob;
   }
   return m_categorizedDeliveries;
}

bool SOLVER::TSPManager::getVehiclesData()
{
   return IO::IOHandler::readVehiclesData(m_fileName, m_numberOfVehicles, m_capacity);
}

bool SOLVER::TSPManager::getDeliveryData()
{
   return IO::IOHandler::readDeliveryData(m_fileName, m_deliveries);
}

bool SOLVER::TSPManager::markAsDepotAndPickUpPoint()
{
   if (m_deliveries.size() <= 0)
      return false;
   // Mark the first place as Depot.
   m_deliveries[0].setType(BASE::DeliveryType::DEPOT);
   m_depot = &m_deliveries[0];

   // We just go through the task list and mark some of them as pickup task randomly.
   // We consider %10 of the input tasks as pickup and the rest will be deliveries.   
   m_numberOfPickups = m_deliveries.size() / 10;
   int jump = m_deliveries.size() / m_numberOfPickups;

   int counter = 1;
   while (counter < m_deliveries.size())
   {
      m_deliveries[counter].setType(BASE::DeliveryType::PICKUP);
      counter += jump;
   }

   return true;
}


