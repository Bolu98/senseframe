from API_network import initReceiver

# Initialises receiver for WebPage and generates logs

# Current setup : A -> node4
#                 B -> node2
#                 C -> node3

user = "senseapp"
password = "ttn-account-v2.cjkyDxuXq-hUK4yaEYGozpOrAcH1byuua5UNR14Yr5w"

initReceiver(user, password)