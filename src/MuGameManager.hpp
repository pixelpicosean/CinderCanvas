//
//  MuGameManager.hpp
//  Mural
//
//  Created by Sean on 10/22/15.
//
//

#pragma once

namespace mural {

  class MuGameManager {
    public:
      virtual void init() = 0;
      virtual void update() = 0;
      virtual void draw() = 0;
  };

}
