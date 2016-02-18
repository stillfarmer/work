url: http://standards.ieee.org/about/get/802/802.html

3. Definitions, acronyms, and abbreviations

    3.1 Definitions

        (1) access domain: A set of stations in an IEEE 802® network together with interconnecting data transmission
                    media and functional units (e.g., repeaters), in which the stations use the same medium access control
                    (MAC) protocol to communicate over a common physical medium

        (2) bridge: A functional unit that interconnects two or more IEEE 802® networks that use the same data link
                    layer (DLL) protocols above the medium access control (MAC) sublayer, but can use different MAC
                    protocols. Forwarding and filtering decisions are made on the basis of layer 2 information

        (3) end station: A functional unit in an IEEE 802® network that acts as a source of, and/or destination for, link
                    layer data traffic carried on the network.

        (4) Ethernet: A communication protocol specified by IEEE Std 802.3TM.

        (5) repeater: A device that interconnects segments of the physical medium by retransmitting a copy of the
                    physical layer (PHY) frame.

        (6) single access domain: A set of stations such that, at most, only one can transmit at a given time, with all
                    other stations acting as (potential) receivers.



4. Family of IEEE 802 standards

5. Reference Models

    5.2 RM descripition for end stations


        5.2.2 LLC sublayer

        5.2.3 MAC sublayer

            Principal functions of the MAC sublayer comprise the following:

                (1) Frame delimiting and recognition;
                (2) Addressing of destination stations (both as individual stations and as groups of stations)
                (3) Conveyance of source-station addressing information
                (4) Transparent data transfer of PDUs from the next higher sublaye
                (5) Protection against errors, generally by means of generating and checking frame check sequences
                (6) Control of access to the physical transmission medium.


    5.3 Interconnection and interworking

        PHY            --- repeaters and hubs
        MAC            --- bridges
        Network-layer  --- routers

        5.3.2 MAC-sublayer interaction: Bridges


            5.3.2.1 Bridges and bridged IEEE 802 networks

                    Bridges are stations that interconnect multiple access domains.


            5.3.2.2 Bridge relaying and filtering

                    A bridge processes protocols in the MAC sublayer and is functionally transparent to LLC sublayer and
                    higher layer protocols.

                    Figure 7.


            5.3.2.3 Resolving topologies with multiple paths
                
                    A key aspect of IEEE Std 802.1D and IEEE Std 802.1Q is the specification of the rapid spanning tree
                    protocol (RSTP), which is used by bridges to configure their interconnections in order to prevent looping
                    data paths in the bridged IEEE 802 network. 

            5.3.2.4 Transparent bridging
            
                   802.1D and 802.1Q pecify transparent bridging operation, so called because the MAC
                   bridging function does not require the MAC user frames transmitted and received to carry any additional
                   information relating to the operation of the bridging functions; end-station operation is unchanged by the
                   presence of bridges.

            5.3.2.5 Provider bridging

                   802.1Q specifies the method by which the MAC service is supported by virtual bridged LANs, the
                   principles of operation of those networks, and the operation of VLAN-aware bridges, including
                   management, protocols, and algorithms.


            5.3.2.6 Bridging example

                    Figure 8.


6. General requirements for an IEEE 802 network

    6.2 Error ratios

        a) For wired or optical fiber physical media: Within a single access domain, the probability that a
           transmitted MAC frame (excluding any preamble) is not reported correctly at the PHY service inter-
           face of an intended receiving peer MAC entity, due only to operation of the PHY, shall be less than
           8 * 10-8 per octet of MAC frame length.

        b) For wired physical media with frames shorter than 2048 octets: The probability that an MAC service
           data unit (MSDU) delivered at an MSAP contains an undetected error, due to operation of the MAC
           service provider, shall be less than 5 * 10-14 per octet of MSDU length.

    
    6.3 Transient service interruption



8. MAC addresses

    8.1 Terms and notional conventions
        In this standard, the term MAC address is used to refer to a 48-bit or 64-bit number that is used to identify
        the source and destination MAC entities. A MAC address may also be used to identify a MAC SAP. In many
        IEEE 802 standards, the term MAC address refers only to a 48-bit MAC address. In some IEEE 802
        standards, the term extended address is used to refer to a 64-bit MAC address.






                

















