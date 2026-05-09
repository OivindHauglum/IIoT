# The-Visualizer-for-hands-on-understanding-

The Visualizer was developed as an IIoT training platform, but the development process
revealed a broader potential that extends well beyond its original scope.
In future versions, The Visualizer can expand its possibilities in the market. After reflection,
it became clear that a product like this could have made a real difference during engineering
studies at USN. Textbooks are great — but hands-on interaction and direct visualisation of
what we work with is better [14, 13]. Laboratory work is great if you have a teacher that get
involved. Two teachers stood out doing this. Henning Gundersen and Jørn Lage Breivoll.
While having access to hardware and software through USN, which gives the opportunity
so synthesize and build, still many of the programs are big, expensive and require quite a bit
storage and computing power. Students struggle to jump through the hoops needed for install
- som many lectures gets degraded and the course gets delayed - personally I have fallen of
some times while waiting for downloads.
The Visualizer is intended for classroom use. It’s versatile and cheap enough to stock,
even for small institutions that struggles to pay licenses for vast arrays of expensive enterprise
software. Parts can be changed with ease and the ESP32 can be preprogrammed and quickly
swapped for different types of use.
This is especially true in signal processing, where the concepts are non-physical by nature.
The graphs are not entirely foreign — heart monitors in films and television, frequency spectra
on oscilloscopes in old technical shows — these are familiar enough to create recognition. But
recognition is not understanding. The formulas in Proakis and Salehi [41] and Oppenheim and
Schafer [42] remain intimidating even after years.
It was with that in mind that the signal processing visualisation tools were developed
alongside this project. It started with revisiting signal processing theory, and Claude Sonnet 4.6
(Anthropic) offered to visualise something rather than explain it in text. The result was four
interactive HTML applications — an interactive unit circle, a z-plane pole-zero diagram, an
IIR frequency response plot, and a live sensor signal with adjustable filter coefficients. Slider
bars. Real-time response. Concepts from the first year with mathematics that all engineering
departments at USN must attend, through to more niche lectures in DSP — suddenly more
tangible.
From this, the idea was born: to build something that fits more purposes than IIoT. With
the right investment, this can actually become a large platform — aiding students of all ages
and disciplines, with the right software and sensors installed. This is embedded systems at its
most accessible.
The ideas emerged late on a Friday evening, while searching through old material from
Sigmund Gudvangen’s TSE2260-1 Communication course — a person the author had a very
good connection with, and whose recommendations shaped the reading list. The lecture notes
on circular buffers were not found. But other things surfaced from OneDrive: PDFs downloaded
years earlier, books recommended by Sigmund and Antonio L.L. Ramos — Xiong [43], Carlson
and Crilly [44], Proakis and Salehi [41], and Couch [45]. Skimming through them again was,
frankly, frightening. The density of the formulas has not changed, it is the same books. What
has changed is the question: what if a student could see this instead of just read it?
The Visualizer can do this. Not by giving answers, but by pushing students to understand
and discover. The author is too old to be misled — real answers are expected, not abstraction
for its own sake.
12.1.1 Transcendence — On the Fear of Change ØH |
This is not a moralistic argument. It is a pragmatic one.
Resistance to change in educational methods is not new, and it is not irrational. Every
generation of educators has faced the same tension: the tools that worked before are known
quantities, and new approaches carry risk. Textbooks have worked for decades. Lectures have
worked for centuries. The formulas in Proakis, Carlson and Xiong are correct — they will
remain correct regardless of how they are taught.
But correctness and comprehension are not the same thing.
The word transcendence — from the Latin transcendere, to climb over — describes precisely
what effective education must do: help the student climb over the wall between formula and
understanding.[46] That wall is real. It was real when the author encountered these same
textbooks years ago, and it remains real for every student who opens them today.
History is consistent on this point: those who adapt fastest to new tools do not merely
survive — they define the next standard. Those who stand still, fall behind — not because
they are wrong, but because the world does not wait. The principle is Darwinian in spirit,
if not in direct attribution: those most responsive to change are those most likely to remain
relevant [47]. The Visualizer is one small argument for adaptation. It does not replace the
textbook. It makes the textbook accessible [14, 13].
With the tools available today — to layman and professional alike — openness and pragmatism in the face of change is not naive optimism. It is engineering. We build better solutions
to make life better, not simply faster. The rest — politics, corporations, pace of adoption — is
beyond the scope of natural curiosity. And natural curiosity, in the author’s experience, is the
one thing that cannot be engineered away.
What began as an IIoT demonstrator for vocational training and industrial operator education may, with the right investment and software development, become a general-purpose
embedded learning platform — serving engineering students from first-year mathematics to
graduate-level signal processing, and extending to self-directed learning at any age.
That idea was born late on a Friday evening, looking at old books. It is the author’s personal
opinion that this is worth pursuing
