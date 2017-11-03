
from abc import ABC, abstractmethod


class Config(ABC):

	@abstractmethod
	def __init__(self, properties: dict=None):
		pass

