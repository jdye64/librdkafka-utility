#!/usr/bin/env bash

conda env create --name kafka_dev --file conda/environments/kafka_dev.yml
source activate kafka_dev