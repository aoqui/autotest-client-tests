from autotest_lib.client.bin import test, autotest_utils
from autotest_lib.client.common_lib import utils


class isic(test.test):
	version = 2

	# http://www.packetfactory.net/Projects/ISIC/isic-0.06.tgz
	# + http://www.stardust.webpages.pl/files/crap/isic-gcc41-fix.patch

        def initialize(self):
		self.job.setup_dep(['libnet'])

	def setup(self, tarball = 'isic-0.06.tar.bz2'):
		tarball = autotest_utils.unmap_url(self.bindir, tarball,
		                                   self.tmpdir)
		autotest_utils.extract_tarball_to_dir(tarball, self.srcdir)
		os.chdir(self.srcdir)

		utils.system('patch -p1 < ../build-fixes.patch')
		utils.system('PREFIX=' + self.autodir + '/deps/libnet/libnet/ ./configure')
		utils.system('make')

	def execute(self, args = '-s rand -d 127.0.0.1 -p 10000000'):
		utils.system(self.srcdir + '/isic ' + args)
