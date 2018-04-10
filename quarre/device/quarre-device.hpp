#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <quarre/interaction/quarre-interaction.hpp>

using namespace ossia::net;

namespace score     {
namespace addons    {
namespace quarre    {

class User // ----------------------------------------------------------- USER
{
    friend class quarre::Device;

    public:
    enum class Status
    {
        DISCONNECTED        = 0,
        IDLE                = 1,
        INCOMING            = 2,
        ACTIVE              = 3,
        INCOMING_ACTIVE     = 4
    };

    class Input // ----------------------------------------------------- USER_INPUT
    {
        public :
        Input ( std::string id );

        private:
        node_base* m_available;
        node_base* m_active;
    };

    User ( uint8_t id );

    bool supports_input         ( std::string input ) const;
    bool connected              ( ) const;
    uint8_t interaction_count   ( ) const;

    void set_connected          ( );

    void activate_input         ( const std::string& input );
    void deactivate_input       ( const std::string& input );

    int active_countdown        () const;

    quarre::Interaction* incoming_interaction   () const;
    quarre::Interaction* active_interaction     () const;

    void set_incoming_interaction   ( quarre::Interaction* interaction );
    void set_active_interaction     ( quarre::Interaction* interaction );

    Status status() const;

    protected: //---------------------------------------------

    bool                    m_connected;
    uint8_t                 m_id;
    node_base*              m_root_node;
    parameter_base*         m_active_countdown;
    uint8_t                 m_interaction_count;
    Status                  m_status;
    quarre::Interaction*    m_incoming_interaction;
    quarre::Interaction*    m_active_interaction;
};

struct Candidate // --------------------------------------------- CANDIDATE
{
    User*       user;
    uint8_t     priority;
};

class Device final : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    public: //------------------------------------------------
    static score::addons::quarre::Device* instance();
    static score::addons::quarre::Device* instance ( const Device::DeviceSettings& settings );

    ~Device ( );

    virtual bool reconnect  ( ) override;
    virtual void recreate   ( const Device::Node& n ) override;

    void   make_tree        ( );

    void   dispatch_incoming_interaction    ( quarre::Interaction* interaction );
    void   dispatch_active_interaction      ( quarre::Interaction* interaction );

    signals: // ----------------------------------------------
    void sig_command    ( );
    void sig_disconnect ( );

    private slots: //-----------------------------------------
    void slot_command();

    private: //-----------------------------------------------
    Device     (const Device::DeviceSettings& settings );

    Device*    m_singleton;
    uint16_t   m_wsport;
    uint16_t   m_oscport;
    uint8_t    m_n_max_users;

    std::vector<quarre::User> m_users;

};
}
}
}
