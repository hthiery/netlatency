# Netlatency

![Build Status](https://github.com/kontron/netlatency/workflows/compile_and_test/badge.svg)


The netlatency toolset is used to measure the latency and jitter parameters
of an ethernet connection. The nl-tx generates UDP packets with
embedded system timestamp (tx-user) and sequence number. The netlantency-rx
captures these packets and dumps the collected receiving information such
as timestamp from the linux network stack and the receiving system time
(rx-user). nl-rx can detect receiving errors (dropped packets or
sequence error).


The following timestamp values can be accessed on the nl-rx.

| Timestamp          | Description                                           |
| ------------------ | ----------------------------------------------------- |
| interval-start     | The interval start timestamp                          |
| tx-wakeup          | The wakeup timestamp of nl-tx                         |
| tx-program         | The timestamp when calling the send function          |
| tx-kernel-netsched | Linux kernel timestamp SOF_TIMESTAMPING_TX_SCHED      |
| tx-kernel-driver   | Linux kernel timestamp SOF_TIMESTAMPING_TX_SOFTWARE   |
| rx-hardware        | Linux kernel timestamp SOF_TIMESTAMPING_RX_HARDWARE   |
| rx-program         | Timestamp when handling the testpacket in nl-rx       |


For linux kernel timestamp please refer to the kernel documentation:

[https://www.kernel.org/doc/Documentation/networking/timestamping.txt](https://www.kernel.org/doc/Documentation/networking/timestamping.txt)

# Build

 * libglib2.0-dev
 * libjansson-dev

## Shortcomings

There is no byte order translation. Therefore, the sender and receiver
application must run on the same CPU architecture.

## nl-tx

### Synopsis

    Usage:
      nl-tx [OPTION...] DEVICE - transmit timestamped test packets

    Help Options:
      -?, --help            Show help options

    Application Options:
      -d, --destination     Destination MAC address
      -h, --histogram       Create histogram data
      -i, --interval        Interval in milli seconds (default is 1000msec)
      -c, --count           Transmit packet count
      -m, --memlock         Configure memlock (default is 1)
      -P, --padding         Set the packet size
      -p, --prio            Set scheduler priority (default is 99)
      -Q, --queue-prio      Set skb priority
      -v, --verbose         Be verbose
      -V, --version         Show version inforamtion and exit

    This tool sends ethernet test packets.

### Outputs

    // not implemented yet
    {
      "type": "settings",
      "object": {
        "interval": 1000,
      }
    }

## nl-rx

### Synopsis

    Usage:
      -rx [OPTION...] DEVICE - receive timestamped test packets

    Help Options:
      -?, --help          Show help options

    Application Options:
      -v, --verbose       Be verbose
      -q, --quiet         Suppress error messages
      -c, --count         Receive packet count
      -s, --socket        Write packet results to socket
      -h, --histogram     Write packet histogram in JSON format
      -e, --ethertype     Set ethertype to filter(Default is 0x0808, ETH_P_ALL is 0x3)
      -f, --rxfilter      Set hw rx filterfilter
      -p, --ptp           Set hw rx filterfilter
      -V, --version       Show version inforamtion and exit

    This tool receives and analyzes incoming ethernet test packets.


### Output

    {
      "type": "rx-packet",
      "object": {
        "sequence-number": 1,
        "packet-size": 64,
        "tx-user-timestamp": "",
        "tx-user-target-timestamp": "",
        "rx-hw-timestamp" "",
        "rx-user-timestamp" "",
      }
    }

    {
      "type": "rx-packet",
      "object": {
        "sequence-number": 1,
        "stream-id": result->stream_id,
        "interval-usec": result->interval_usec,
        "offset-usec": result->offset_usec,
        "packet-size": result->packet_size,
        "timestamps": {
          "names": [
            "interval-start":,
            "tx-wakeup",
            "tx-program",
            "tx-kernel-netsched",
            "tx-kernel-driver",
            "rx-hardware",
            "rx-kernel-driver",
            "rx-program",
          ]
          "values": [
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
            <TIMESTAMP>,
          ],
        }
      }
    }

    {
      "type": "rx-error",
      "object": {
        "dropped-packets": 1,
        "sequence-error": true,
      }
    }


    // not implemented yet
    {
      "type": "rx-packet-tx-timestamp",
      "object": {
        "sequence-number": 1,
        "tx-hw-timestamp": "",
      }
    }

## ETF - Earliest TxTime First Qdisc

When using the etf option of nl-tx make sure the qdisc configuration is as
required.

Example:

    tc qdisc del dev ${IFACE} root
    tc qdisc add dev ${IFACE} parent root mqprio num_tc 3 map 2 2 1 0 2 2 2 2 2 2 2 2 2 2 2 2 queues 1@0 1@1 2@2 hw 0
    MQPRIO_NUM=`tc qdisc show dev ${IFACE} | grep mqprio | cut -d ':' -f1 | cut -d ' ' -f3`
    tc qdisc add dev ${IFACE} parent ${MQPRIO_NUM}:1 etf clockid CLOCK_TAI delta 150000 offload

## Helper: nl-calc

The nl-calc tool stores the testpacket results of nl-rx and builds information
for histogram analysis. Including Application accurancy/latency, full transmit
latency (from application to RX hardware) and rx jitter.

| Histogram type  | Description                                                                            |
| --------------- | -------------------------------------------------------------------------------------- |
| program-latency | Histogram data of program wakeup time. It is used to observe the application accuracy. |
| scheduled-times | Histogram data of packets runtime.                                                     |
| jitter          | Histogram data of packets runtime in a smaller window with nanosecond resolution.      |

## Helper: nl-report

The nl-report tool displays the data generated by nl-calc in graphs.


## Helper: nl-trace

The nl-trace displays the output by nl-rx in a box-plot graphic. Each type of
timestamp is collected and the data depicted. Hence a time distribution of the
latency till the point of record can be seen.


For closer information about meaning of box-plot take a look at:

[https://en.wikipedia.org/wiki/Box_plot](https://en.wikipedia.org/wiki/Box_plot)

## Usage Examples


### Receive testpackets and send with socat to UDP port

On receive servers site:

    $ nl-rx enp2s0  -v |  socat - udp-sendto:127.0.0.1:5000


On client site:

    $ socat - udp4-listen:5000,reuseaddr,fork


### Receive testpackets, calc latency, generate histogram and plot in file

    $ nl-rx enp2s0 -c 10000 -v | nl-calc -  | nl-report - /tmp/plot.png
