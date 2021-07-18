#pragma once

#include "BasicOperations.h"

namespace SOLVER
{
   class HeuristicTSP: public BasicOperations
   {
   public:
      HeuristicTSP(std::vector<BASE::DeliveryPoint> deliveries, int capacity, int numberOfVehicles, int numberOfPickups);
      ~HeuristicTSP() = default;

      virtual auto execute() ->std::pair<std::vector<BASE::DeliveryPoint>, int> override;
   private:
      //! tries to distribute tasks to vehicles heuristically.
      void distributeDeliveries();

      std::vector<BASE::DeliveryPoint> m_copyOfDeliveries;
      bool m_problemSolved = false;
   };
}

