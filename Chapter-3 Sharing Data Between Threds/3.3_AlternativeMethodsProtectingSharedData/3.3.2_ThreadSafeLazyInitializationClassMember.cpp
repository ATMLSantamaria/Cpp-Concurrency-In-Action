
#include <mutex>

class data_packet{};
class connection_info {};
class connection_handle {
public:
  void send_data(const data_packet data){}
  data_packet receive_data(){return data_packet();}
};
class manager {
public:
  connection_handle open(connection_info c_info){return connection_handle();}
};

class X {
private:
  connection_info connection_details;
  connection_handle connection;
  manager connection_manager;

  std::once_flag connection_init_flag;

  void open_connection() {
    connection = connection_manager.open(connection_details);
  }
public:
  X(connection_info const & connection_details_) : connection_details(connection_details_){}

  void send_data(data_packet const& data) {
    // here comes the call_once
    std::call_once(
      connection_init_flag,    // Flag to track initialization state
      &X::open_connection,     // Pointer to the member function to call
      this                     // The object instance to call it on (`this` = current X object)
    );
    
    connection.send_data(data);
  }

  data_packet receive_data() {
    std::call_once(
      connection_init_flag,    // Flag to track initialization state
      &X::open_connection,     // Pointer to the member function to call
      this                     // The object instance to call it on (`this` = current X object)
    );
    
    return connection.receive_data();
  }

};

// The initialization is done by the first call of send_data() or receive_data()

// They use call_once and open_connection() to assure initialization is done just once.

// call_conce require this pointer to be passed

// NOTA: instances of std::once_flag cannot be copied or moved.