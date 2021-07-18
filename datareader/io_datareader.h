#pragma once

#include <string>
#include <vector>

namespace BASE
{
   class DeliveryPoint;
}

namespace IO
{
   class IOHandler
   {
   public:
      //! using these methods we can read data from mentioned file and add them to our problem set.
      static bool readDeliveryData(std::string &fileName, std::vector<BASE::DeliveryPoint>& deliveries);
      static bool readVehiclesData(std::string &fileName, int &numberOfVehicle, int &capacity);
   };
}