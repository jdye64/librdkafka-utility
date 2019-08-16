#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <rdkafkacpp.h>

class ExampleEventCb : public RdKafka::EventCb {
public:
    void event_cb (RdKafka::Event &event) {
        switch (event.type())
        {
            case RdKafka::Event::EVENT_ERROR:
                if (event.fatal()) {
                    std::cerr << "FATAL ";
                }
                std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                          event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                fprintf(stderr, "LOG-%i-%s: %s\n",
                        event.severity(), event.fac().c_str(), event.str().c_str());
                break;

            default:
                std::cerr << "EVENT " << event.type() <<
                          " (" << RdKafka::err2str(event.err()) << "): " <<
                          event.str() << std::endl;
                break;
        }
    }
};

void msg_consume(RdKafka::Message* message, void* opaque) {
    const RdKafka::Headers *headers;

    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            std::cout << "Read msg at offset " << message->offset() << std::endl;
            if (message->key()) {
                std::cout << "Key: " << *message->key() << std::endl;
            }
            headers = message->headers();
            if (headers) {
                std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
                for (size_t i = 0 ; i < hdrs.size() ; i++) {
                    const RdKafka::Headers::Header hdr = hdrs[i];

                    if (hdr.value() != NULL)
                        printf(" Header: %s = \"%.*s\"\n",
                               hdr.key().c_str(),
                               (int)hdr.value_size(), (const char *)hdr.value());
                    else
                        printf(" Header:  %s = NULL\n", hdr.key().c_str());
                }
            }
            printf("%.*s\n",
                   static_cast<int>(message->len()),
                   static_cast<const char *>(message->payload()));
            break;

    }
}


int main (int argc, char **argv) {

    // Program operational variables
    std::string errstr;

    // read_kafka_options that should be supplied by cython
    std::string brokers = "localhost:9092";
    std::string topic_str = "jeremy-test";
    int32_t partition = 0;
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;

    // Instance variables that should be set for the consumer
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    conf->set("client.id", "Ast", errstr);
    conf->set("enable.partition.eof", "true", errstr);

    RdKafka::Consumer *consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
        //TODO: CUDF_EXPECT HERE ....
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }

    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, topic_str,
                                                   tconf, errstr);
    if (!topic) {
        //TODO: CUDF_EXPECT HERE ....
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }

    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        //TODO: CUDF_EXPECT HERE ....
        std::cerr << "Failed to start consumer: " <<
                  RdKafka::err2str(resp) << std::endl;
        exit(1);
    }

    RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
    msg_consume(msg, NULL);
    delete msg;
    consumer->poll(0);

    /*
     * Stop consumer
     */
    consumer->stop(topic, partition);


    delete topic;
    delete consumer;
    delete conf;
    delete tconf;

    return 0;
}