# TCC
Este repositório é dedicado ao TCC apresentado ao curso de Ciência da Computação da Universidade do Estado de Santa Catarina (UDESC), como requisito parcial para a obtenção do grau de bacharel em Ciência da Computação. O repositório contempla um fork do projeto SOCIABLE desenvolvido pelo laboratório LUMO da UFPB, com alterações na função RelayElection.

## Clonar a pasta do projeto
`make copy-sociable`

## Criar imagem docker
`make build-docker-image`

## Descompactar o .zip com os arquivos .tcl
`minuet/utils/trace/tcl/TCC.zip`

## Abrir container
`make start`  
`make exec`

## Rodar a simulação (dentro do container)
`make configure`  
`make build`  
`make run`  
`make rundb` (modo debug)

