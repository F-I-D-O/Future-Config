
from abc import ABC, abstractmethod


class Config(ABC):

	@abstractmethod
	def fill(self, properties: dict=None):
		pass

