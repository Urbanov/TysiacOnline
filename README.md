# TysiacOnline

[bardzo stary niekatualny opis]

W tym momencie klient dziala jak prosty czat - kazdy gracz przy polaczeniu z serwerem dostaje kolejne id, a przy probie dolaczenia do pokoju jest przypisywany do pierwszego wolnego; w kazdym pokoju moze byc 3 graczy i kazdy pokoj ma osobny czat. Bedac w pokoju, wyslana wiadomosc dojdzie jedynie do pozostalych graczy znajdujacych sie w tym samym pokoju. Po stronie serwerowej logiki gry znajduje sie juz wiecej funkcjonalnosci zwiazanych z cala gra, jednak nie zostala jeszcze zaimplementowana pelna komunikacja server<->client.

W projekcie znajduja sie przykladowe testy z Boost::Test, w tym momencie jedynie dla logiki gry, uruchamiaja sie przy wybraniu targetu 'Test' w projekcie VS.

Projekt wymaga bibliotek boost (korzysta z boost::asio), Beast, ktora jest header-only nakladka na asio dodajaca obsluge websocket oraz nlohmann::json, ktora takze jest header-only i umozliwia parsowanie JSON. Obie te biblioteki znajduja sie w server/TysiacServer/lib/.

