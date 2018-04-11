#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <quarre/interaction/quarre-interaction.hpp>

using namespace ossia::net;

using pdata_t   = std::pair<std::string,ossia::val_type>;
using parptr_t  = std::shared_ptr<parameter_base>;

namespace score     {
namespace addons    {
namespace quarre    {

using intact_t = std::shared_ptr<quarre::Interaction>;

class user // ----------------------------------------------------------- USER
{
    public:
    enum class status
    {
        DISCONNECTED        = 0,
        IDLE                = 1,
        INCOMING            = 2,
        ACTIVE              = 3,
        INCOMING_ACTIVE     = 4
    };

    class input // ----------------------------------------------------- USER_INPUT
    {
        public :
        input ( std::string id );

        void assign     ( std::string const& id, std::function<void(ossia::value&)> function );
        void unassign   ( std::string const& id );

        private:
        std::string     m_id;
        std::string     m_addr;
        parptr_t        m_available;
        parptr_t        m_active;

        std::vector<parptr_t> m_data;
    };

    class gesture : public input //--------------------------------------- gesture
    {
        public:
        gesture ( std::string id, std::vector<std::string> subgestures );
    };

    class sensor : public input //--------------------------------------- sensor
    {
        public:
        sensor ( std::string id, std::vector<pdata_t> data );
    };

    user ( uint8_t id, device_base* device );

    void make_tree ( std::vector<pdata_t> const& tree );

    bool supports_input         ( const std::string& input ) const;
    void activate_input         ( const std::string& input );
    void deactivate_input       ( const std::string& input );

    bool connected              ( ) const;
    void set_connected          ( );

    status status() const;

    protected: //---------------------------------------------
    bool                            m_connected;
    uint8_t                         m_id;
    status                          m_status;
    std::vector<input*>             m_inputs;

    class interaction_hdlr
    {
        public:
        int active_countdown            ( ) const;
        intact_t incoming_interaction   ( ) const;
        intact_t active_interaction     ( ) const;
        uint8_t interaction_count       ( ) const;

        void set_incoming_interaction   ( intact_t interaction );
        void set_active_interaction     ( intact_t interaction );
        void cancel_next_interaction    ( intact_t interaction );
        void stop_current_interaction   ( intact_t interaction );
        void end_current_interaction    ( intact_t interaction );
        void pause_current_interaction  ( intact_t interaction );
        void resume_current_interaction ( intact_t interaction );

        protected:
        intact_t m_incoming_interaction;
        intact_t m_active_interaction;
        parptr_t m_active_countdown;
        uint8_t  m_interaction_count;

    } m_interaction_hdl;
};

class Device final : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    public: //--------------------------------------------------------------------------------

    static score::addons::quarre::Device* instance();
    static score::addons::quarre::Device* instance ( const Device::DeviceSettings& settings );
    static score::addons::quarre::Device::dispatcher const& dispatcher();
    static const std::unique_ptr<ossia::net::device_base>& device();

    ~Device ( );

    virtual bool reconnect  ( ) override;
    virtual void recreate   ( const Device::Node& n ) override;

    void   make_common_tree ( );

    signals: // ----------------------------------------------
    void sig_command    ( );
    void sig_disconnect ( );

    private slots: //-----------------------------------------
    void slot_command();

    private: //-----------------------------------------------
    Device     (const Device::DeviceSettings& settings );

    Device*             m_singleton;
    uint16_t            m_wsport;
    uint16_t            m_oscport;
    uint8_t             m_n_max_users;

    class dispatcher // -----------------------------------------------------------------
    {
        public:
        void   dispatch_incoming_interaction    ( intact_t interaction );
        void   dispatch_active_interaction      ( intact_t interaction );
        void   dispatch_ending_interaction      ( intact_t interaction );
        void   dispatch_paused_interaction      ( intact_t interaction );
        void   dispatch_resumed_interaction     ( intact_t interaction );

        private:
        struct candidate // --------------------------------------------- candidate
        {
            user*       user;
            uint8_t     priority;
        };

        std::vector<quarre::user> m_users;

    } m_dispatcher;

};
}
}
}
